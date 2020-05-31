program BranchTest;

function branch(i : integer) : integer;
begin
    if i = 0 then
	begin
		writeln('a');
	end
	else
	begin
        case i of 
            1: writeln('b');
            2: writeln('c');
        end
        ;
	end
	;
end
;

begin
    branch(0);
    branch(1);
    branch(2);
end
.