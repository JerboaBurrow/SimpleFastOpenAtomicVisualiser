if (record == nil) then
    record = true
    sfoav.startRecording()
    sfoav.play()
end

if (sfoav.getFrame() == 10) then
    sfoav.stopRecording()
    sfoav.pause()
end