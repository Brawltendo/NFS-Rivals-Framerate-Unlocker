# NFS Rivals Framerate Unlocker

This plugin aims to unlock Need for Speed Rivals' framerate above 30 FPS while keeping the gameplay intact. Older methods involving solely launch commands caused major gameplay issues that this mod seeks to correct.

# How to use

- Open Origin/EA App and navigate to NFS Rivals' game properties, then go to the advanced launch options.
  - Pick `Need for Speed Rivals (x64)` from the dropdown, as this mod only works with the 64-bit version of the game.
  - Copy these commands into the command line arguments box: `-disable-client-correction false -Client.VisualFrameInterpolation true -GameTime.MaxVariableFps 200 -RenderDevice.VsyncEnable 0 -Ant.InterpolatePoses false`
    - Feel free to change `GameTime.MaxVariableFps` to whatever value above 30 FPS you want, and you can also remove `RenderDevice.VsyncEnable 0` if you prefer vsync to be enabled.
- Download the latest release from the `Releases` section on this page, and copy the contents of the archive to NFS Rivals' root directory.
- Download the latest version of Frosty Mod Manager from [here](https://github.com/CadeEvs/FrostyToolsuite/releases), launch it, and add NFS Rivals as a game. Then go to File->Pack->Import, and import `RivalsUnlocked.fbpack` file from the `Scripts` folder in your Rivals directory. Click `Apply Mods(s)`, and from there you should be able to launch the game through Frosty with everything working as intended.

# Special Thanks
- EA/Criterion Games/Ghost Games for developing and publishing this game.
- [TsudaKageyu](https://github.com/TsudaKageyu) for the tried and true [MinHook](https://github.com/TsudaKageyu/minhook).
- [cursey](https://github.com/cursey) for the elegant [SafetyHook](https://github.com/cursey/safetyhook).
- [ThirteenAG](https://github.com/ThirteenAG) for the classic [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader).

This project would not exist without the hard work of all of these people.
