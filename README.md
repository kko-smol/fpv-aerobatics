# fpv-aerobatics

FPV-Aerobatics - AR project, where you fly on your plane or quad by virtual track with checkpoints. 
 + H264 DVR.

Hardware:
  - orange pi zero or other embedded linux system with support V4L2 and EGL\GLES2 GPU
  - USB UVC camera(or other v4l2 device with yuyv plane output)
  - Autopilot with support Mavlink telemetry
 
 
 Dependencies:
  - EGL, GLES2 driver for you GPU
  - boost
  - GLM
  - pthread
  - ffmpeg(x264,cedarx,z,mp3lame)
