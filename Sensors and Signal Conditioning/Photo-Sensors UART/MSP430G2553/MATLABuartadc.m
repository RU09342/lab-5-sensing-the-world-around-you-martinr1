function MATLABuartadc
 
% This function opens a serial connection to the MSP430 Launchpad attached
% via USB. You need to change the device address to match the address on
% your system. The function sends a 'D' command to initiate the data
% acquisition on the MSP430 and then reads in the data that is sent back.
 
Nstring = 14;
Nintdata = 248;
obj=serial('COM3','BaudRate', 9600,'Terminator', 'LF/CR','InputBufferSize',2*Nintdata+Nstring);
fopen(obj);
finishup = onCleanup(@() fclose(obj));
command = 'D';
fprintf(obj,command);
string = fscanf(obj,'%c',14);
disp(['Response: ' string(1:end-2)])
 
if strcmp(string(1:end-2),'Sending data'),
data = fread(obj,Nintdata,'uint16');
 
N=length(data);
disp([num2str(N) ' integers received'])
Fs = 10e3/0.044
time = (0:N-1)./Fs;
figure(1),plot(data,'-o')
xlabel('Time (s)')
ylabel('Amplitude')
end
 
freq = [0:N/2-1,-N/2:-1].*(Fs/N);
Data = fft(data);
figure(2), plot(freq(1:N/2),20*log10(abs(Data(1:N/2))))
xlabel('Frequency (Hz)')
ylabel('Spectral Magnitude (dB)')