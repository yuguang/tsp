f = csvread('perf15.csv');
m = min(f,[],2);
r = [f(:,1)./m, f(:,2)./m, f(:,3)./m, f(:,4)./m];

r_prune = [];
m_prune = [];

for i = 1:size(r)
    if any(isnan(r(i,:)))
        continue
    else
        r_prune = [r_prune; r(i,:)];
        m_prune = [m_prune; m(i)];
    end
end

gridpoints = logspace(0,log10(max(max(r_prune))),50);

Enum = [];
OT = [];
BC = [];
BHK = [];

co = [  0    0.4470    0.7410;
    0.8500    0.3250    0.0980;
    0.9290    0.6940    0.1250;
    0.4940    0.1840    0.5560;
    0.4660    0.6740    0.1880;
    0.3010    0.7450    0.9330;
    0.6350    0.0780    0.1840];
set(groot,'defaultAxesColorOrder',co)

for g = gridpoints
    Enum = [Enum; sum(r_prune(:,1) <= g)];
    OT = [OT; sum(r_prune(:,2) <= g)];
    BC = [BC; sum(r_prune(:,3) <= g)];
    BHK = [BHK; sum(r_prune(:,4) <= g)];
end

figure(1);
h = semilogx(gridpoints,Enum,'-',gridpoints,OT,'-',gridpoints,BC,'-',gridpoints,BHK,'-','LineWidth',2);
set(h(1),'Color',co(1,:));
set(h(2),'Color',co(2,:));
set(h(3),'Color',co(3,:));
set(h(4),'Color',co(4,:));
xlim([1 max(max(r_prune))]);
xlabel('Factor Slower Than Best','FontSize',16);
ylabel('Number of Instances','FontSize',16);
title('15 Cities','FontSize',16);
legend('Enumeration','HK 1-Trees','Branch & Cut','BHK Dynamic Programming','Location','SE');