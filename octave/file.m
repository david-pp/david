#!/bin/octave -qf

function data = loaddata(fieldname, columns, del=",")
  file = fopen("data.txt", "r");
  while (!feof(file))
    line = fgetl(file);
    values = strsplit(line, del);
    if (length(values) == 3)
      str2double(values{1,1})
      data = []
    end  
  end
  fclose(file);
end

loaddata("data.txt", 3)

