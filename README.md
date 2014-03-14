xor_timelock
============

This is a PoC for **Antiemulation Through Time-lock Puzzles**:

http://people.csail.mit.edu/rivest/pubs/RSW96.pdf (article on timelock-puzzles)
http://www.moluch.ru/archive/37/4202/ (russian article by me :) it provides some testing results on a **real malware** scope)


Source file information:
------------------------

**Packer.cpp** - App used to produce timelock and inject it into Unpacker
**Unpacker.cpp** - Container which calculates timelock, extracts encrypted data, and runs it

Timelock details:
----------------
**data**, represented by malware file, is descrypted by simple XOR-ing it with a long **key**.
**key** is calculated as: **key** = <em>a</em>^(<em>2</em>^<em>t</em>) <em>(mod n)</em>, where <em>n</em> = <em>p</em>*<em>q</em> and
<em>p,q</em> are large enough primes; <em>t</em> is a variable "time" parameter.

In theory, such calculation of <em>a</em> is <em>one-way</em>, that is, it can be simply calculated by the producing party, 
and is irreversible by third party: given an <em>f(x)</em> you cannot estimate <em>x</em> argument (in our terms - **key**) in any acceptable period of time (simply put, it is VERY long to estimate).

Payloaded executable file, on execution, tries to unpack the encrypted malware file by performing long operation of exponentiation (see above definition of **a**).
On success, it writes the decrypted stuff to a disk and execute it.

**What we try to do** is determine, if such a timelock gets executed by antivirus emulator.

