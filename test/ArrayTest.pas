program ArrayTest;
const
	l = 1;
	r = 6;
var
	iv : integer;
	a : array[l..r] of integer;

begin
	iv := 0;  
	for iv := l to r do
	begin
		a[iv] := iv;
	end;
	for iv := l to r do
	begin
		writeln(a[iv]);
	end;
end
.
