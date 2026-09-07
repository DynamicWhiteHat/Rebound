import os
import glob
from collections import deque
import numpy as np
import pandas as pd
from scipy.stats import mode
from scipy.fft import rfftfreq, rfft
from scipy.signal import find_peaks
from scipy.signal import correlate

# 1. Setup Parameters
fs = 2000
window_ms = 200
window_size = int((window_ms/1000)*fs)
long_window_ms = 1500
long_window_size = int((long_window_ms/1000)*fs)
half_long = long_window_size // 2
overlap = 0.5
step = int(window_size * (1 - overlap))
envelope_rate = 20
min_period_s = 0.5
max_period_s = 1.5

# Master storage
all_features = []

file_patterns = [
    "Data/P*_S*/*_swallow_water.csv",
    "Data/P*_S*/*_swallow_banana.csv",
    "Data/P*_S*/*_swallow_dry.csv",
    "Data/P*_S*/*_cough.csv",
    "Data/P*_S*/*_speech.csv"
]

# Combine files
file_list = []
for pattern in file_patterns:
    file_list.extend(glob.glob(pattern))

print(f"Found {len(file_list)} target files to process.\n")

for file_path in file_list:
    # Extract Patient ID automatically from the folder name
    folder_name = os.path.basename(os.path.dirname(file_path))
    patient_id = folder_name.split('_')[0] 

    trial_type = (
    "water" if "water" in file_path
    else "banana" if "banana" in file_path
    else "dry" if "dry" in file_path
    else "cough" if "cough" in file_path
    else "speech" if "speech" in file_path
    else "unknown"
    )
    
    data = pd.read_csv(file_path)
    sEMG = data.iloc[:, 0].to_numpy()
    if not np.isfinite(sEMG).all():
        sEMG = pd.Series(sEMG).interpolate(limit_direction='both').to_numpy()
    classification = data.iloc[:, 5].to_numpy()
    
    prev_rms = 0  

    recent_peaks = deque(maxlen=8)
    
    for i in range(0, len(sEMG) - window_size+1, step):
        end = i + window_size
        window = sEMG[i:end]
        # Create long window
        center = i + window_size // 2
        long_start = max(0, center - half_long)
        long_end = min(len(sEMG), center + half_long)
        long_window = sEMG[long_start:long_end]

        window_labels = classification[i:end]

        if 2 in window_labels:
            label = 2
        else:
            mode_result = mode(classification[i:end], keepdims=False)
            label = int(np.asarray(mode_result.mode).item()) if hasattr(mode_result, 'mode') else int(mode_result)
        
        # RMS
        rms = np.sqrt(np.mean(window**2))

        # Waveform length
        # Basically this takes the derivative of the signal array, takes the absolute values of each derivative, and sums them together. Measure of muscle activation!
        waveform = np.sum(np.abs(np.diff(window)))

        # Zero Crossing Rate
        zcr = np.nonzero(np.diff(np.sign(window)))[0].size / window_size
        
        # Mean/Median Frequency
        fft = np.abs(rfft(window))
        freqs = rfftfreq(window_size, 1/fs)
        fft_sum = np.sum(fft)
        
        if fft_sum > 1e-9:
            mean_freq = np.sum(freqs * fft) / fft_sum
            cumulative_sum = np.cumsum(fft)
            
            median_idx = np.searchsorted(cumulative_sum, fft_sum / 2)
            median_freq = freqs[median_idx]
        else:
            mean_freq = 0
            median_freq = 0

        # UPDATE: Include max amplitude + burst count + max onset slope + rms delta
        clean = np.abs(window)
        envelope = np.convolve(clean, np.ones(envelope_rate)/envelope_rate, mode='same')
        peaks = find_peaks(envelope, prominence=np.max(envelope)*0.3)
        burst_count = len(peaks[0])
        max_amplitude = np.max(clean)

        if len(recent_peaks) > 0:
            rel_prom = max_amplitude/np.mean(recent_peaks)
        else:
            rel_prom = 1.0
        recent_peaks.append(max_amplitude)
        # Slope from point 0 to the max amplitude point
        loc = np.argmax(envelope)
        max_onset_slope = (envelope[loc] - envelope[0]) / (loc + 1) if loc > 0 else 0
        rms_delta = rms - prev_rms
        prev_rms = rms

        # Next update: Include Willison Amplitude
        wamp_threshold = 0.05 * np.max(np.abs(sEMG[0:len(sEMG)//10]))
        willison_amplitude = np.sum(np.abs(np.diff(window)) >= wamp_threshold)

        # Next next update: Long window features
        clean2 = np.abs(long_window)
        envelope2 = np.convolve(clean2, np.ones(envelope_rate)/envelope_rate, mode='same')

        # Autocorrelation
        ac = correlate(envelope2, envelope2, mode='full')
        ac = ac[len(ac)//2:]
        ac = ac/(ac[0] + 1e-9)  # Normalize
        min_lag = int(min_period_s * fs)
        max_lag = int(max_period_s * fs)
        search_region = ac[min_lag:max_lag]

        ac_peak = np.max(search_region) if len(search_region) > 0 else 0

        # Burst
        peaks2 = find_peaks(envelope2, prominence=np.max(envelope2)*0.3)
        burst_count2 = len(peaks2[0])

        # Peak falloff
        window_max = np.argmax(envelope)
        global_loc = i+ window_max
        end_loc = min(global_loc + int(0.5*fs), len(sEMG)-1)
        if end_loc <= global_loc:
            peak_falloff = 0
        else:
            peak_falloff = (envelope[window_max] - np.abs(sEMG[end_loc])) / (end_loc - global_loc)

        all_features.append({
            'id': patient_id,
            'trial_type': trial_type,
            'RMS': rms, 
            'Waveform': waveform, 
            'ZCR': zcr, 
            'MeanFreq': mean_freq, 
            'MedianFreq': median_freq,
            'MaxAmplitude': max_amplitude,
            'RelProminence': rel_prom,
            'BurstCount': burst_count,
            'MaxOnsetSlope': max_onset_slope,
            'RMSDelta': rms_delta,
            'WillisonAmplitude': willison_amplitude,
            'AutocorrPeak': ac_peak,
            'BurstCountLong': burst_count2,
            'PeakFalloff': peak_falloff,
            'label': label
        })

# Save master file
final = pd.DataFrame(all_features)
final.to_csv("final_master.csv", index=False)
