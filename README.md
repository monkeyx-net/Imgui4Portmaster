## Welcome


To build this version run make in src.

Changes can be made in main.cpp the key section that create the windows

```ImGui::Begin("Portmaster Test",&p_open, window_flags);```

This closes the window created

```ImGui::End();```

This is a basic template for use in portmaster

makefile runs strip automatically to reduce file size.

![](https://github.com/monkeyx-net/Imgui4Portmaster/blob/master/imgui-controls-v6-Xbox.png?raw=true)

TODO
- [x] Create template
- [x] Build on aarch64
- [x] Test on my RGB30 Jelos and ArkOS
- [ ] Create CLI arguments for title and text
- [ ] Create CLI arguments for buttons
- [x] Check full screen and scaling
- [ ] Parse json files