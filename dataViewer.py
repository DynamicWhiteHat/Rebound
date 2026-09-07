import matplotlib.patches as patches
import matplotlib.pyplot as plt
import numpy as np

# 1. Load sEMG and Class labels
semg_data, class_labels = np.loadtxt(
    "Data/P4_S1/01_swallow_banana.csv",
    delimiter=",",
    usecols=(0, 5),
    unpack=True,
)

# 2. Signal Parameters & Envelope Calculation
fs = 2000  # Sampling rate in Hz
envelope_rate = 20  # Moving average window size in samples
time = np.arange(len(semg_data)) / fs

rectified = np.abs(semg_data)
envelope = np.convolve(
    rectified, np.ones(envelope_rate) / envelope_rate, mode="same"
)

# 3. Sliding Window Parameters
window_ms = 200
window_size = int((window_ms / 1000) * fs)  # 400 samples
overlap = 0.5
step = int(window_size * (1 - overlap))  # 200 samples
window_sec = window_ms / 1000.0

# 4. Create Figure with Subplots (Main Plot + Bottom Window Track)
fig, (ax1, ax2) = plt.subplots(
    2,
    1,
    figsize=(14, 6),
    sharex=True,
    gridspec_kw={"height_ratios": [5, 1]},
)

# --- TOP PANEL: Clean sEMG Plot with Class Shading ---
unique_classes = np.unique(class_labels)
class_colors = plt.colormaps["tab10"].resampled(len(unique_classes))

change_indices = np.where(np.diff(class_labels) != 0)[0] + 1
boundaries = np.concatenate(([0], change_indices, [len(class_labels) - 1]))

for i in range(len(boundaries) - 1):
    start_idx, end_idx = boundaries[i], boundaries[i + 1]
    cls = int(class_labels[start_idx])
    cls_idx = list(unique_classes).index(cls)

    ax1.axvspan(
        time[start_idx],
        time[end_idx],
        color=class_colors(cls_idx),
        alpha=0.20,
        zorder=1,
        label=f"Class {cls}" if i < len(unique_classes) else "",
    )

ax1.plot(
    time,
    envelope,
    color="black",
    linewidth=1.0,
    label="sEMG Envelope",
    zorder=2,
)

ax1.set_title("sEMG Envelope with Class Shading")
ax1.set_ylabel("Amplitude (mV)")
ax1.grid(True, alpha=0.3)

# Consolidate duplicate legend entries
handles, labels = ax1.get_legend_handles_labels()
by_label = dict(zip(labels, handles))
ax1.legend(by_label.values(), by_label.keys(), loc="upper right")

# --- BOTTOM PANEL: Window Track Bar under the X-Axis ---
num_windows = (len(envelope) - window_size) // step + 1

# Base background container rectangle for the track
track_bg = patches.Rectangle(
    (0, 0.05),
    time[-1],
    0.9,
    facecolor="#f2f2f2",
    edgecolor="black",
    linewidth=1,
    zorder=1,
)
ax2.add_patch(track_bg)

# Draw staggered window rectangles and tick marks along the track
for i in range(num_windows):
    start_t = (i * step) / fs

    # Alternating heights inside the track bar to clearly show 50% overlap
    y_pos = 0.50 if (i % 2 == 0) else 0.10
    height = 0.40

    rect = patches.Rectangle(
        (start_t, y_pos),
        window_sec,
        height,
        linewidth=0.8,
        edgecolor="#0044cc" if (i % 2 == 0) else "#cc4400",
        facecolor="#6699ff" if (i % 2 == 0) else "#ff9966",
        alpha=0.75,
        zorder=2,
    )
    ax2.add_patch(rect)

    # Window start tick mark along the bottom ruler
    ax2.vlines(
        x=start_t,
        ymin=0,
        ymax=1,
        color="gray",
        linewidth=0.5,
        linestyle="--",
        alpha=0.4,
        zorder=3,
    )

# Track formatting
ax2.set_ylim(0, 1)
ax2.set_xlim(0, time[-1])
ax2.set_yticks([])  # Remove y-ticks for a clean ruler bar
ax2.set_ylabel("Windows", rotation=0, ha="right", va="center")
ax2.set_xlabel("Time (seconds)")

plt.subplots_adjust(hspace=0.08)  # Tight gap between signal and window track
plt.tight_layout()
plt.show()