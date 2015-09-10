%Script to calculate and plot a rectified sine wave
 t=linspace(0,10,100);
 y=abs(sin(t)); % The abs command makes all negative numbers positive
 plot(t,y);
 title('Rectified Sine Wave');
 xlabel('t');