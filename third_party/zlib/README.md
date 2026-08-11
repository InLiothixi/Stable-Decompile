# zlib

This directory contains the portable core of zlib 1.3.2, vendored from the
official release tag at <https://github.com/madler/zlib/tree/v1.3.2>.

The canonical zlib 1.3.2 source archive is available from
<https://zlib.net/zlib-1.3.2.tar.gz> with SHA-256:

`bb329a0a2cd0274d05519d61c667c062e06990d72e125ee2dfa8de64f0119d16`

Only the portable compression, decompression, and checksum sources used by
the game and its PNG loader are included. The gzip file API, examples, tests,
build systems, and architecture-specific optimizations are intentionally
omitted. See `LICENSE` for the upstream license.
