function incrementVars(sys)
tos=find_system(sys,'BlockType','ToWorkspace');
%froms=find_system(sys,'BlockType','FromWorkspace');
froms=[];

for i=1:length(tos)
    cblock=tos(i);
    cblock=cblock{:};
    name=incrementName(cblock);
end

for i=1:length(froms)
    cblock=froms(i);
    cblock=cblock{:};
    name=incrementName(cblock);
end

function newname = incrementName(block)
    name=get_param(block,'VariableName');

    val=1;
    i=0;
    newname=name;
    while ((newname(end) >= '0') && (newname(end) <= '9'))
        val=val+str2num(newname(end))*10^i;
        i=i+1;
        newname=newname(1:end-1);
    end
    newname=[newname num2str(val)];
    set_param(block,'VariableName',newname);