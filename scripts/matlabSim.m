close all
clear all
clc

% Define parameters
maxC = 310
minC = 120
DL = 180;
delta = 40;
inrate = 5;
outRatePercent = [70    50     38     30     38     57    115     140     144     135     118     102     100     98     98     98     105     120     140     148     142     130     110     80]
outrate = floor((inrate * outRatePercent)/100)

n = 10000;

% Initialize variables
invalve = 1;
CL = DL - delta;
tick = 0;
CL_values =zeros(1,24*n);
tick_values = zeros(1,24*n);
sensorReading = CL;
% Simulate system for 48 ticks
while tick < 24*n
    % Check if invalve should be turned on/off
    if mod(tick,2) == 0
        if sensorReading >= DL + delta
            invalve = 1;
        elseif sensorReading <= DL - delta
            invalve = 0;
        end
        sensorReading = CL;
    end
    % Add inrate to CL if invalve is on
    if invalve
        CL = CL - inrate;
    end
    outrate_tick = outrate((mod(tick,24))+1); % Get outrate for current tick
    CL = CL + outrate_tick;

    % Save CL value and increment tick
    CL_values(tick+1) = CL;
    tick_values(tick+1) = tick;
    tick = tick + 1;
end

maxL = max(CL_values)
minL = min(CL_values)

% Plot CL values
plot(tick_values, CL_values);
xlabel('Tick');
ylabel('CL');
title('CL Values over Time');

figure
pdlevels = fitdist(CL_values', "Normal");
plot(pdlevels);

figure
histogram(CL_values)

times = zeros(1,24*n);
levels = zeros(1,24*n);
for i = 1:length(CL_values)
    levels(i)=CL_values(i);
    t =0;
    if CL_values(i) > DL+delta
        j = i+1;
        while(j <= length(CL_values))
            t = t + 1;
            if(CL_values(j) < DL+delta )
                break
            end
            j=j+1;
        end
    end
    times(i)=t;
end

figure
plot(times);
hold on;
grid on;
plot(levels);


figure
pdtimers = fitdist(times(times~=0)', "Gamma");
plot(pdtimers);
grid on, hold on
plot(ecdf(times(times~=0)))
figure
histogram(times(times~=0));


maxt = max(times)
mint = min(times(times~=0))

