program GotoTest;

var 
    i : integer;

begin
    i := 2;
0:
    writeln('a');
    i := i - 1;
    if i > 0 then
    begin
        goto 0;
    end
    ;

end
.