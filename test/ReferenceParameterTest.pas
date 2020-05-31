program ReferenceParameter;

var 
    i : integer;

function modify(var n : integer) : integer;
begin
    n := 1;
end
;

begin
    i := 0;
    writeln(i);
    modify(i);
    writeln(i);
end
.