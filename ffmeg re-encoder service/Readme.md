# How to setup

- copy `ffmpeg-radio` and it's contents to `/root`
- copy `systemd/ffmpeg-radio.service` to `/etc/systemd/system`
- auto start service by `systemctl enable ffmpeg-radio`
- run service by `systemctl start ffmpeg-radio`