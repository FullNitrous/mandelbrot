ffmpeg \
-r 15 \
-f image2 \
-s 1920x1080 \
-i img/%d.png \
-vcodec libx264 \
-crf 25 \
-pix_fmt yuv420p \
vid/vid.mp4