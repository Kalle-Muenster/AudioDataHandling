# AudioDataHandling
Library for handling noncompressed, multichannel Audio data which can be build as either:

- native c++ lib
- dll for dotnet48
- dll for dotnet50

*there also projects for building a dll linking against qt exists, and as well project which builds on linux and on windows by gcc /mingw compiler exist.. but since I didn't had much time for maintainance, these are not in some really 'up to date' states actually. so for now these are not contained within this repo actualy..  

### Supported file formats for loading / storing buffers:

- Windows (wav)
- Sun audio (au, snd)
- NetPbm (pam) ...quite uncommon format for transporting audiodata.. but turned out it's really handy format for this.. (especially due to it's possible with this storing 16bit float pcm data as audio files) see: http://netpbm.sourceforge.net/doc/pam.html
 

    
