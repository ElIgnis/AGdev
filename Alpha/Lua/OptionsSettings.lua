--[[  Load options for game]]--

--Check number of options
function readDataSize(fileName)
f = assert(io.open(fileName, "r"))
local count = 0
	for _ in io.lines 'Lua\\Options.Lua' do
		count = count + 1
	end
	f:close()
	return count
end

--Save options
function writeData(brightness, vSense, hSense)
f = assert(io.open("Lua\\Options.Lua", "w"))
f:write(brightness, "\n")
--Round off numbers before saving
roundOff(vSense, 2)
roundOff(hSense, 2)
f:write(vSense, "\n")
f:write(hSense, "\n")
f:close()
end

--Read options
function readData(fileName, lineNum)
f = assert(io.open(fileName, "r"))

local count = 1
	while true do
		line = f:read();
		if line == nil then break
		end
		if count == lineNum
			then f:close()
			return line
		end
		count = count + 1
	end
end

--Rounding for precision
function roundOff(number, precision)
	return tonumber(string.format("%." .. (precision or 0) .. "f", number))
end