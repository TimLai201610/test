hide := @
ECHO := echo

.PHONY: all
all: AlsaRecordTest AlsaPlayTest
	$(hide)$(ECHO) "Build All Done ..."

AlsaRecordTest:
	$(hide)$(ECHO) "Build AlsaRecordTest ..."
	./../../../buildroot/output/host/usr/bin/arm-rockchip-linux-gnueabihf-gcc lrecord.c wav_parser.c sndwav_common.c -lasound -o AlsaRecordTest
	$(hide)$(ECHO) "Build AlsaRecordTest Done ..."

AlsaPlayTest:
	$(hide)$(ECHO) "Build AlsaPlayTest ..."
	./../../../buildroot/output/host/usr/bin/arm-rockchip-linux-gnueabihf-gcc lrecord.c wav_parser.c sndwav_common.c -lasound -o AlsaPlayTest
	$(hide)$(ECHO) "Build AlsaPlayTest Done ..."

