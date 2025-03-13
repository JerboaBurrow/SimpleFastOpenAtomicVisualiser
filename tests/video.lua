if (record == nil) then
    record = true
    sfoav.toggleRecord()
    sfoav.play()
end

if (sfoav.getFrame() == 10) then
    sfoav.toggleRecord()
    sfoav.pause()
end