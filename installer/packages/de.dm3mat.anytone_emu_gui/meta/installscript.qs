/****************************************************************************
 **
 ** Copyright (C) 2024 Hannes Matuschek, DM3MAT
 **
 ****************************************************************************/

function Component() {
    // default constructor
}

Component.prototype.createOperations = function() {
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut",
                               "@TargetDir@/anytone-emu-gui.exe", "@StartMenuDir@/anytone-emu-gui.lnk",
                               "iconPath=@TargetDir@/anytone-emu-gui.ico",
                               "description=AnyTone Emulator");
    }
}
