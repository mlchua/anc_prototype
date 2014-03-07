% reset everything
clear all; close all; clc;

% wait for button press
wait = 0;

% generate input
input_size = 1000;
input = 2048 .* sin(1:input_size)' + 2048;
input = uint16(input);

% create input buffer
buffer_size = 5;
buffer = zeros(buffer_size,1);

% create weights and randomize values
weights_size = 5;
weights = 2 .* rand(weights_size,1) -1;

% set step size
step = 0.000000003;

% create vectors to hold values for predicted and error values
predicted_values = zeros(input_size,1);
error_values = zeros(input_size,1);

% distance buffers
in_dist_values = zeros(input_size,1);
pr_dist_values = zeros(input_size,1);

for ii = 1:input_size
    % read in next value
    buffer(:) = [input(ii); buffer(1:buffer_size-1)];
    
    % predict next value
    predict = buffer' * -weights;
    
    % get error
    in_dist = input(ii) - 2048;
    pr_dist = predict - 2048;
    error = double(in_dist) + double(pr_dist);
    
    % print out all values
    fprintf('Input = %6f \nPredicted = %6f \nError = %6f \n\n', ...
        input(ii), predict, error);
    if wait == 1
        k = waitforbuttonpress;
    end
    
    % calculate new weights based on error
    weights(:) = (1-step) .* weights + step * error .* double(buffer);
    
    % save predicted values and error values
    predicted_values(ii) = predict;
    error_values(ii) = error;
    
    % save distance values for input and prediction
    in_dist_values(ii) = in_dist;
    pr_dist_values(ii) = pr_dist;
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


