# RGZ Crypto Project

Минимальный крипто-проект с реализацией XTEA, ChaCha20 и Rabbit.

## Build

```bash
make
```

## Run

```bash
./prog -k -xtea key_file               # Generate XTEA key (16 bytes)\n"
./prog -k -cc20 key_file               # Generate ChaCha20 key (32 bytes)\n"
./prog -k -rbbit key_file              # Generate Rabbit key (16 bytes)\n"
./prog -e -xtea input_file key_file    # Encrypt with XTEA\n"
./prog -e -cc20 input_file key_file    # Encrypt with ChaCha20\n"
./prog -e -rbbit input_file key_file   # Encrypt with Rabbit\n"
./prog -d -xtea input_file key_file    # Decrypt with XTEA\n"
./prog -d -cc20 input_file key_file    # Decrypt with ChaCha20\n"
./prog -d -rbbit input_file key_file   # Decrypt with Rabbit\n";
```

## Exemple

```bash
./prog -k -xtea key

Enter password to generate key: pwd
Key generated and saved to output/key
```

```bash
./prog -e -xtea test/img.jpg output/key

Encrypted to output/img.jpg.enc
```

```bash
./prog -d -xtea output/img.jpg.enc output/key

Decrypted to output/img_dec.jpg
```
