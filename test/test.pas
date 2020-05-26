program hello;
const
	ic = 1;
	l = 1;
	r = 6;
	rc = 1.5;
	bc = false;
	cc = 'a';
var
	iv : integer;
	rv : real;
	bv : boolean;
	cv : char;
	p : integer;
	a : array[l..r] of integer;

begin
	iv := 0;  
	for iv := l to r do
	begin
		a[l + 1] := iv + 1;
		writeln(a[l + 1]);
	end;
	a[l + 1] := 8;
end
.
