
function Component()
{
}
Component.prototype.createOperations = function()
{
    component.createOperations();
    if (installer.value("os") === "win") {
        var viewerPath = "Z:/24AM/Analytics/Development/CLOModel2/Environment/CLOViewer.exe";
        component.addOperation("RegisterFileType",
                               "clo",
                               viewerPath + " '%1'",
                               "CLO model output file",
                               "application/cloresult",
                               viewerPath + ",0",
                               "ProgId=TwentyFourAM.CLOModel.clo");
		component.addOperation("RegisterFileType",
                               "fcsr",
                               viewerPath + " '%1'",
                               "CLO model stress test",
                               "application/clostress",
                               viewerPath + ",0",
                               "ProgId=TwentyFourAM.CLOModel.fcsr");
    }
}


