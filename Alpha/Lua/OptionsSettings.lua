--[[  Save options for game]]--

--Save options
function writeData(brightness, vSense, hSense)
f = assert(io.open("Lua\\OptionsConfig.Lua", "w"))
f:write("Brightness = ", brightness, "\n")
--Round off numbers before saving
vSense = roundTo2DP(vSense)
hSense = roundTo2DP(hSense)
f:write("VSense = ", vSense, "\n")
f:write("HSense = ", hSense, "\n")
f:close()
end

--Rounding for precision
function roundTo2DP(number)
	return tonumber(string.format("%.2f" , number))
end