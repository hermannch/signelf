# SignElf

Signelf is a toolset written in C++ for signing elf binaries, specifically linux
shared object, though in theory, any elf binary may be signed. It uses openssl's
libcrypt to RSA sign the library, and the same to validate the signature.

## Introduction

I suppose you're curious why someone would want to sign an elf binary? 
Suppose you have an application that uses plugins. If each of those
plugins were signed, you could be assured that each was coming from
a trudsted source, unmodified, and uncorrupted. For secure systems
this can be very important. Signelf uses RSA signing through openssl's
libcrypt library. RSA key pairs are generated, along with a signing
tool, and static library to link into your application. You can validate
shared objects before dynamically loading them with a simple function
call. To sign a shared object (or any elf binary) you simply run the 
signing app. It will take the data and text sections of the binary, 
and sign them with the keypair that was created, then append that
signature to the shared object as another elf section.


## Installation

```
make
make install
```


## Usage

### Signing

from a prompt, run: `signelf <path/to/shared/object.so>`

### Verifying

link in libsignelf.a

call this:
```
#include "signelf.h"
#include "pubkey.h"

bRetval = signelf::verifyLib(szKeyBuf, sizeof(szKeyBuf), "/path/to/shared/object.so");
```

szKeyBuf is the public key, and it is defined in pubkey.h.


## License

LGPL-2.1-or-later


## Origin

Original author: Joe Fox <joe@codenoise.com>

Original repository: https://sourceforge.net/projects/signelf/

The repo showed this additional text:
```
Acknowledgements

Big thanks to Doug Keith and Pete Birch. Huge thanks to the openssl guys for
making libcrypt, and providing a solid interface for strong encryption! Also a
big thanks for Linspire, Inc for sponsoring this development.

Drop me a line at: joe@codenoise.com if you find this useful!
Thanks

-Joe
```
