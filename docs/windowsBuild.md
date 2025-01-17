# Windows Build Instructions

Use cd to get into the folder where this package is, generally:
```powershell
cd ~\.pulsar\packages\pulsar-mouse-buttons
```

Once there you will need to ensure that the mouseDetect dependency is built, to do this
cd into it
```powershell
cd .\lib\mouseDetect
```

and run
```powershell
node-gyp configure
```

If you encounter any errors, you should make sure you installed properly (don't forget to restart your computer after installing everything)

Next run
```powershell
node-gyp build
```

then run
```powershell
npm build
```

then go back to the main package directory with
```powershell
cd ..\..
```

and run
```powershell
npm build
```
