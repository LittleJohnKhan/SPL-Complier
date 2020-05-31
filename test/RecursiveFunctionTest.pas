program RecursiveFunctionTest;

var
    i : integer;

function Fibonacci(n : integer) : integer;
begin
    if n = 0 then
    begin
        Fibonacci := 0;
    end
    else 
    begin
        if n = 1 then
        begin
            Fibonacci := 1;
        end
        else
        begin
            Fibonacci := Fibonacci(n - 1) + Fibonacci(n - 2);
        end
        ;
    end
    ;
end
;

begin
    for i := 1 to 10 do
    begin
        writeln(Fibonacci(i));
    end
    ;
end
.