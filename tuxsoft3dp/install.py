# installer for the TuxSoft3DP driver
# Copyright 2017-2018 Arthur Emerson, vreihen@yahoo.com
# Distributed under the terms of the GNU Public License (GPLv3)

from setup import ExtensionInstaller

def loader():
    return TuxSoft3DPInstaller()

class TuxSoft3DPInstaller(ExtensionInstaller):
    def __init__(self):
        super(TuxSoft3DPInstaller, self).__init__(
            version="1.03",
            name='TuxSoft3DP',
            description='Capture data from TuxSoft 3D Printed station in weatherflow UDP broadcast packets',
            author="Arthur Emerson",
            author_email="vreihen@yahoo.com",
            files=[('bin/user', ['bin/user/tuxsoft3dp.py'])]
            )

