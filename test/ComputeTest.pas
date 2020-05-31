program buildInTypeTest;
const
	ic = 1;
	rc = 1.5;
    jc = 8;
    kc = 3;
begin
    writeln(ic + ic);
    writeln(rc + rc);
    writeln(ic - ic);
    writeln(rc - rc);
    writeln(ic * ic);
    writeln(rc * rc);
    writeln(ic / (ic + 1));
    writeln(jc mod kc);
end
.
