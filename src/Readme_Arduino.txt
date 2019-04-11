To Get the WROOM 02 to work with the new Arduino IDE (1.8+) I had to modify the WEMOS D1 R1 baord config in boards.txt

Find ..
		d1_mini.build.flash_freq=40

Add this After
		d1.menu.eesz.2M=2M (no SPIFFS)
		d1.menu.eesz.2M.build.flash_size=2M
		d1.menu.eesz.2M.build.flash_size_bytes=0x200000
		d1.menu.eesz.2M.build.flash_ld=eagle.flash.2m.ld
		d1.menu.eesz.2M.build.spiffs_pagesize=256
		d1.menu.eesz.2M.upload.maximum_size=1044464
		d1.menu.eesz.2M.build.rfcal_addr=0x1FC000

will be before
		d1_mini.menu.eesz.4M=4M (no SPIFFS)


If you do this you will be able to use OTA
