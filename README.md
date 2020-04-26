### This repository is still in a WIP state and it's far from being completed or usable

# PSD
PSD is a *Man in the middle* attack to the security of the Pokemon Sword And Shield LAN Mode.
Its objective is the *on the fly* manipulation of packets flowing through two Different Nintendo Switch during an in game trade (performed in LAN mode).

Using this technique one could easily manipulate packets to inject modified Pokémon into the savefile using non-modded hardware.

## Performing the attack
As of now the attack only exists theoretically and this implementation is not ready yet. You may want to test the work done so far by building this repository and using the software.

The attack cannot analyze network traffic yet, so it works by scanning a *.pcapng* file that contains packets traded between consoles (you can obtain this file by analyzing your network traffic with a sniffing software such as **Wireshark** while trading two Pokémon with the game set in LAN mode).

As an output you will get a *.pcapng* file in the Document folder of your computer that contains the manipulated network traffic.

You can use the software by launching it via command line specifying this **mandatory** options:

- -a ADDRESS: *The Nintendo Switch IP Address you want to inject the Pokémon in*
- -t TYPE: *The network interface type you want to perform the attack on, as of now you may wish to specify the **pcap** option*
- -n FILEPATH: *The path of the .pcapng file you wish to scan*
- -i INPUTFILE: *The .ek8 file path containing the Pokémon you wish to inject*

Launching the program with the *-h* option will provide some more informations on how to use this software

## Integration with Switch Lan Play
If you feel **really** brave you can try the experimental integration with Switch Lan Play, it will let you perform the attack using two real Nintendo consoles trading a Pokémon via a simulated LAN over the internet (you can find more informations on Switch Lan Play [here](https://github.com/spacemeowx2/switch-lan-play))

To use this mode you will need to set both consoles as you would normally do when using Switch Lan Play (more informations [here](http://www.lan-play.com/install) under the **Switch Configuration** section)
you will then need to compile a special modified version of Switch Lan Play, its source is provided as a submodule of this repository.

#### Please note: 
- Only the console you are performing the attack on needs to connect using the modified SLP executable, the other one may as well use the standard executable.
- You don't need to launch the newly compiled executable manually (in fact you shouldn't), put it in the same folder as the PSD executable, it will be launched automatically

To use this mode you will need to use this command line options:

- -a ADDRESS: *The Nintendo Switch IP Address you want to inject the Pokémon in*
- -t TYPE: *The network interface type you want to perform the attack on, you will need to use the **slp** option*
- -n SERVERADDRESS: *The Switch Lan Play server address you wish to connect to*
- -i INPUTFILE: *The .ek8 file path containing the Pokémon you wish to inject*

## Building this repository
Although the attack is not totally implemented yet you may wish to compile a preliminary version of this software. In order to compile it you will need:
- Qt5
- libpcap
- openssl

You can then compile the software using the provided *qmake* file

## Special Thanks
A special thanks goes to Yannik Marchand, whose [repository](https://github.com/Kinnay/NintendoClients) made me come with the idea of this attack and its help and documentation was crucial in every part of the developing process

Another huge emotional help came from the VGC Campania community, my local VGC community that sparkled my interest in competitive Pokémon and motivated me to work on this, another one goes the NEST GANG which I shared a lot of kilometers on italian roads with, just to perform badly at tournaments

## Why PSD?
It's an acronym for *Per Sentiro Dire* an italian gergal expression which is generally used when you know something other people don't know but you don't want to tell anyone because you are afraid that something bad will happen to you (i had this developed even before the Swo/Shi launch date, but was afraid to share because i thought it would have been patched)