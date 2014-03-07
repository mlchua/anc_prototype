% reset everything
clear all; close all; clc;

% wait for button press
wait = 0;

% generate input
input_size = 1000;
input = uint16(2048 .* sin(1:input_size)' + 2048);

% create input buffer
buffer_size = 5;
buffer = zeros(buffer_size,1);

% create weights and randomize values
weights_size = 5;
weights = 1 .* rand(weights_size,1);

% set step size
step = 0.000000001;

% create vectors to hold values for predicted and error values
predicted_values = uint16(zeros(input_size,1));
error_values = uint16(zeros(input_size,1));

for ii = 1:input_size
    % read in next value
    buffer(:) = [input(ii); buffer(1:buffer_size-1)];
    
    % predict next value
    predict = buffer' * weights;
    if predict > 4096
        predict = 4096;
    elseif predict < 0 
        predict = 0;
    end
    
    % get error
    error = input(ii) - predict;
    
    % print out all values
    fprintf('Input = %6f \nPredicted = %6f \nError = %6f \n\n', ...
        input(ii), predict, error);
    if wait == 1
        k = waitforbuttonpress;
    end
    
    % calculate new weights based on error
    weights(:) = (1-step) .* weights + step * double(error) .* buffer;
    
    % save predicted values and error values
    predicted_values(ii) = predict;
    error_values(ii) = error;
end

% plot subplot of actual values vs predicted values vs error values
fig1 = figure(1);
subplot(3,1,1);
plot(1:input_size, input);
title('Generated Input');
subplot(3,1,2);
plot(1:input_size, predicted_values);
title('Predicted Input');
axis([0 1000 -5000 5000])
subplot(3,1,3);
plot(1:input_size, error_values);
title('Error = Input - Predicted');
axis([0 1000 -5000 5000])


