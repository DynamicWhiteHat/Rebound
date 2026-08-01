---
title: "Rebound"
author: "DynamicWhiteHat"
description: "A Low-Cost Dysphagia Patch for Post-Stroke Patients"
created_at: "2026-07-13"
---

# July 13th: Initial Research and First Schematic

**Research**

Today, I began my project by doing some research into the issue. I found that post-stroke patients can find themselves facing difficulties swallowing, a condition known as dysphagia. This can lead to *silent aspiration*, which is when food enters the airway without choking or other visible signs, and is very dangerous. As a result, patients struggling with dysphagia typically have recurring tests conducted to track their muscle condition and ability to swallow, with individual tests costing upwards of $2000. New research into surface electromyography (sEMG) shows that ICs like the ADS1292 are capable of performing high-quality sEMG analysis. Coupled with the AD5941 bioelectric measurement IC, a PCB could effectively measure a patient's capability to swallow. This measurement would be non-invasive, 24/7, and low-cost, which covers many problems associated with current testing methods. As such, I decided to create a PCB with these ICs.

**Schematic Design**

I began by creating a base schematic for an ESP-32 S3 SoC. I have created many boards with ESP-32 modules, but never just the bare SoC before. I found [this video]([url](https://www.youtube.com/watch?v=yxU_Kw2de08)) from Phil's Lab that covers exactly what I needed. I just pulled up the datasheets for the ESP-32 S3 since he used a C3, made some minor changes, and ended up with the following:

| | |
|---|---|
| <img width="827" height="692" alt="image" src="https://github.com/user-attachments/assets/f0708e9e-0f63-4cb5-a411-13ae308d0f71" /> | <img width="597" height="640" alt="image" src="https://github.com/user-attachments/assets/53ec4fea-67b7-4773-910c-c3562510e08d" /> |
| <img width="1162" height="641" alt="image" src="https://github.com/user-attachments/assets/aa9660d2-8c76-427d-87b3-89552752eb9a" /> | <img width="757" height="317" alt="image" src="https://github.com/user-attachments/assets/507a52a2-0504-4690-a9b1-2f7e42b6482c" />|

At the core is an ESP-32 S3 R8 coupled with a 16 MB flash chip. The battery circuit, mostly taken from my [custom ZMK keyboard]([url](https://github.com/DynamicWhiteHat/Arc/)), features battery charging and USB switchover, as well as a physical power switch and a battery gauge. The antenna is a simple CLC circuit attached to an AN043 antenna, which has been tested and proven to work by Ti. I then added the AD5941 IC. This is a very sensitive analog device that is going to require special routing to ensure the signals are kept safe. I looked at reference designs online and came up with this:

<img width="676" height="642" alt="image" src="https://github.com/user-attachments/assets/91c87434-dd2e-4cd6-801e-1e83d1acc5b1" />

Next, I set up my board for a 4-layer design, following JLCPCB's specifications for their JLC04161H-7628 stackup, which is cheap and good for impedance control. I used KiCad's built in calculator to get a track width of 0.3812 mm for a 50 ohm impedance trace for the antenna.

**Total time spent: 4 hours**

# July 15th: Final Schematic and Initial PCB Layout

**Schematic**

I had initially planned to make a separate board for the ADS1292 IC, which would be positioned higher on the neck. I got this idea from an AI generated mockup of the device. However, I later decided that placing the IC on the same board and using and FPC cable instead would help me with routing and signal concerns. I imported the IC and placed it into my schematic. Before I started making connections, I decided to add some more safety and protection lines to my AD5941 IC, such as an inductor and an ESD protection IC. This is how it looks now:

<img width="932" height="887" alt="image" src="https://github.com/user-attachments/assets/18ca424a-32ea-4c1b-b195-f8a1db232ba0" />

U9 is a TPD4E001 from TI, which is designed to catch any stray leakage. I also lowered the resistor value from 10K to 1K to allow the signals to be better intercepted, as previously, it would be very difficult for the IC to read signals that have gone through a 10K resistor. Finally, I also added inductors on the end of the connection, right before the line goes out to the electrode, to catch any RF/EMI signals and remove them before they reach the IC and corrupt its readings.

Next, I began working on the ADS1292 IC. Similar to how I did the AD5941, I looked up a reference design online and got to work copying it. I found the reference in the TI documentation for the IC and adjusted it to match my use case. The electrode pins on this IC are IN2N, IN2P, and RLDOUT, which connect to an FPC connector. It also has the same electrode setup, with a resistor, ESD protection, and an inductor. This is how it looks:

<img width="937" height="705" alt="image" src="https://github.com/user-attachments/assets/202837b8-aad9-4506-a280-f08db6f0b9d0" />

**PCB**

I then got to work on the PCB and did some initial layout of the IC "groups," which helps me with later placement. For example, here is the IC group layout for the AD5942:

<img width="342" height="752" alt="image" src="https://github.com/user-attachments/assets/307d8c91-e084-4aef-a0ac-cec111e13340" />

I did that with most of the major parts, then made my Edge.Cuts for the outline. It is a rough estimate of the size and will be edited once I finish placement. I decided to place the antenna on the left side, as that will help keep RF signals away from the analog ICs, which will go on the right. This is my current PCB:

<img width="541" height="797" alt="image" src="https://github.com/user-attachments/assets/934bd13e-df0a-49fc-acc1-e3af74ac380f" />

**Total time spent: 3 hours**

# July 20th: More PCB Work

I did some more PCB work today. I added the buttons in the top right and figured out where to put the power section. I decided to move the ESP32 chip down to accommodate the power stuff towards the top; that way it sort of blends in with the rest of the devices for the USB. Afterwards, I added the analog devices on the board, but then I realized that it didn't look good where it was, so I asked in Slack for help and am still awaiting a response. For now, this is where I have it.

<img width="552" height="791" alt="image" src="https://github.com/user-attachments/assets/80d98088-4927-4bb5-82e8-63776b607e8f" />

**Total time spent: 1 hour**

# July 21st: Major PCB Work

Today I got a lot of the PCB layout done. I'm still working on the analog section, but mostly everything else is laid out. 
<img width="457" height="558" alt="image" src="https://github.com/user-attachments/assets/6dd0cbd1-2044-4203-880f-87bb838829d3" />

I began by deleting the bottom extra space after the antenna, as I didn't think that I would need to use that space, and it makes the shape much more pleasing to look at. However, I moved the antenna down a bit before that to accommodate the loss of space that I could have used earlier. I then placed some final components that I forgot about, such as the MAX17048:
<img width="813" height="700" alt="image" src="https://github.com/user-attachments/assets/89fe3f41-664e-4297-8b00-a547e4b4536b" />

I shifted around some of the surrounding ICs to make it align and look better. In the image above, you can also see that I added the connector for the battery and NTC thermistor. I originally didn't know where to put the two, as I thought they would be in the way in their current position and make routing hard. However, I moved the ESP32 and its capacitors down to make space, which fits well.

One of the largest changes I made was to swap out the ZIF connector for a 10-pin JST connector. I have decided that since this board is getting quite large, it would be better to mount it on the back of the neck and run wires to the front electrodes. I have found the following wires on AliExpress, which should work well:

<img width="467" height="328" alt="image" src="https://github.com/user-attachments/assets/29973047-4216-46ee-b0d8-d696b027d782" />

Finally, I also decided to place a shield can over the analog ICs. I tried my best to move them away from RF and digital lines, but they will still end up very close to the antenna. I placed fencing vias around the antenna to block its frequencies from the analog ICs, and I plan to use Harwin shield can clips to house a custom-cut aluminum shield can to prevent any interference from the air.

**Total time spent: 3 hours**
# July 22nd: Finish Placement and Begin Routing

I finally figured out how to fit the two analog ICs. They are still very close to the antenna, and I am relying mainly on the combination of a solid ground pour, fencing vias, and a shield cap to prevent any interference to the devices. I decided to place the ADS1292 furthest from the antenna since it is the most sensitive of the two. I also had to extend the board a little bit on the bottom to allow the resistors and capacitors for the AD5940 to fit, since the IC itself is larger than the ADS1292. I flanked both sides of the JST connector with the ESD protection diodes, and that was all for placement. This is what I have for now:

<img width="582" height="678" alt="image" src="https://github.com/user-attachments/assets/2a0e612b-4cf6-48cf-97a5-c94807d3d962" />

I then got started with routing the PCB. It is pretty difficult since everything is so compact. I decided to start with some of the easier stuff first, like the short connections between two resistors. While routing my flash, I noticed that the ESP32 symbol in my schematic seemed off. Referencing back to my original screenshot, I realized that, at some point, the symbol switched its pins around. I don't know how that happened, but I was able to undo it and copy the original symbol. I have 41 tracks left to route. This is what I have right now:

<img width="542" height="657" alt="image" src="https://github.com/user-attachments/assets/2ba198fc-471d-4c31-bb31-0c26bda78307" />

**Total time spent: 2.5 hours**
# July  23rd: Finish PCB

I finished routing the tracks. There wasn't much in the way of routing, as I just had to make some tweaks to existing tracks and vias to make new ones fit. However, I did have to make a major change in the schematic. While routing my AD5940, I noticed that I accidentally connected the AD5940 interrupt pin to the START pin of the ADS1292. This was incorrect. Up until now, I had been relying mostly on Gemini and Claude while referencing a bit of the documentation for the two ICs since I didn't want to read 160 pages. However, once I noticed this small mistake that both AIs missed, I decided to check the wiring myself. I pulled up the pinouts of the ADS1292 first. I referenced each pin and checked its description to see what it should be connected to. For example, the VREFN pin:
<img width="767" height="27" alt="image" src="https://github.com/user-attachments/assets/d2809e29-7e32-4efd-bac8-a1087fed68ff" />
For other pins, they gave solely a description of what it was for. In such cases, I searched through the datasheet to either find a connection recommendation or a typical layout of it. Once I finished all the pins, this is what I ended up with (not much changed):
<img width="1110" height="830" alt="image" src="https://github.com/user-attachments/assets/93b5e12e-9540-4a75-afed-42b867923eec" />

There was a lot more change for the AD5940 chip. While looking through the datasheet, I found this figure for 4-wire bioimpedance measuring, which is what I am trying to do:

<img width="791" height="516" alt="image" src="https://github.com/user-attachments/assets/aace7ead-a46c-4e5c-b176-a43e4f7567d2" />

This was much different from my schematic, as I was using the RE0, SE0, and DE0 pins instead of AIN1-3. I copied the figure, referenced an existing board for resistor vlaues, and came up with this new design:

<img width="712" height="681" alt="image" src="https://github.com/user-attachments/assets/3df61f9a-fbde-43bf-99b0-197fb3e82b24" />

After that, I moved some resistors around on the board, finished routing, and ended up with this:

<img width="532" height="612" alt="image" src="https://github.com/user-attachments/assets/46f8cd53-4805-4b6f-8677-5cebe5ba2832" />

The white rectangle is temporary; it is to show where the shield cap goes.

**Total time spent: 3.5 hours**

# July 27th: Initial (Final?) Case

The case was a very simple design, as there can't be too much on the back of a patient's neck. This is the design I came up with:
<img width="1147" height="381" alt="image" src="https://github.com/user-attachments/assets/5c3fd138-bd14-439a-b1e0-5c29d0d27437" />
The holes in the ends of the arms are for a Velcro strap. Initially, I had decided to just use a belt system, where the silicone would wrap around your neck and slide under itself, locking it in place. However, I realized that it had the chance of interfering with the larynx/Adam's apple location, so I decided that Velcro would be better, as it can be adjusted to fit better. 

I began the design by creating an offset of the PCB, which I exported from KiCad. The wall is 2mm thick, with a 0.2mm tolerance offset.
<img width="562" height="616" alt="image" src="https://github.com/user-attachments/assets/3642724f-d899-4619-9b33-d803898323a8" />

After I built the wall, I added two tabs, one on the top and one on the bottom, to help keep the PCB in while allowing easy removal to charge the battery. I intentionally left the USB hole covered in the design, that way, you can't charge it while the device is still attached to you, in case anything goes wrong. 

<img width="777" height="547" alt="image" src="https://github.com/user-attachments/assets/8d0f31d7-1487-4c82-a8a2-6974fcd413ca" />

I found a 502035 400mAh battery on Amazon, which will work nicely for this design. It is thin yet holds ample power. After that, I rounded the edges and added the arms on the side, which finished my design. I added a hole for the wiring. This is my preliminary design, and I may adjust it after I actually get the PCB and Velcro.

**Total time spent: 0.75 hours**

# July 28th: Conglomerate of Activities

I began today by looking for some electrodes fit for my design. Due to their location under the chin and on the neck, the electrodes need to be very small, preferrably between 10-20mm in diameter. However, the issue with this size is that they are not *easily* commercially available. Although products like Coviden Kendall H124SG exist (shown below), they are single use and are only available from medical distributors, which incurs a high shipping fee if the order cost does not exceed $200. For this project and its budget, these electrodes are simply not good.

<img width="120" height="120" alt="image" src="https://github.com/user-attachments/assets/20dfdaa0-233a-49d2-a6f1-73251fceaa92" />

I continued my search on AliExpress, looking for small electrodes, pediatric electrodes, and similar items. I did not find anything good, with most of the electrodes being 30mm at the smallest. The smallest I was able to find were part of multi packs, sold as TENS electrodes. These electrodes are reusable and are primarily used for pain relief, but they work just fine for this project. Below is one of the packs:

<img width="237" height="240" alt="image" src="https://github.com/user-attachments/assets/f7a65842-027b-4294-b9cf-03b187ee7e87" />

However, I do not need all of the other sizes, and they only provide a few of the very small size. The best alternative I was able to find was these oddly shaped single-use electrodes:

<img width="428" height="316" alt="image" src="https://github.com/user-attachments/assets/92ab872c-6d41-48df-9bc5-9595e9f3f1f4" />

I continued my search on Amazon. I mainly found more of the TENS electrode packs. After searching for a while and using the Alexa shopping assistant, I was able to find these 30mm electrodes:

<img width="398" height="451" alt="image" src="https://github.com/user-attachments/assets/b08efd31-d321-4656-bae4-5f6173e80118" />

I quickly made some samples out of a napkin I had, and they seem to fit. I will use these for now.

Afterwards, I moved on to the parts selection for my PCB. Many of the components on my PCB require specialized characteristics, such as a high Q factor or ceramic core. I mostly just googled for parts, with the search of "Good [component] for [circuit]," and I used the AI response. I trust the AI on this since they are pretty common parts that are used by many people. I opened the parts on LCSC, copied their LCSC number and added them to my BOM in KiCad. I used the JLCPCB tools plugin for KiCad to generate a BOM. I got tired of this and ended early, after adding most of the important parts. The rest of the components do not require any special considerations.

After that, I moved on to the code. I set up PlatformIO to serve as my codebase. This is my first time using PlatformIO. I chose it to allow me to use multiple cores of the ESP32 while keeping everything in different files, which is best done in VSCode. I watched a few videos on the math that goes into SEMG analysis. The general process is as follows:

- FFT to filter the background noise
- Bandpass filter to remove the background noise
- RMS envelope to find the average power of a frame of SEMG data

Now I have to code it.

**Total time spent: 4 hours**
