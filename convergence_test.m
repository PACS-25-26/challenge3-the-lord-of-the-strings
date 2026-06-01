% Extracted data
N = [16; 32; 64; 128];
errore = [0.00737541; 0.00171521; 0.000414652; 0.000101998];

% Evaluation of h
h = 1 ./ (N - 1);

% Fitting linearly in log-log space to find the experimental order of convergence
p = polyfit(log(h), log(errore), 1);
pendenza_sperimentale = p(1);

% Printing the experimental order of convergence
fprintf('=========================================\n');
fprintf('Experimental Slope (Order): %.4f\n', pendenza_sperimentale);
fprintf('=========================================\n');

% --- CONVERGENCE PLOT ---
figure('Color', 'w');

% Plotting data
loglog(h, errore, 'o-', 'LineWidth', 2, 'MarkerSize', 8, 'Color', [0.85 0.55 0.10], 'DisplayName', 'Error L_2 Experimental');
hold on;

% Generation of the theoretical convergence line (O(h^2))
errore_teorico = h.^2 * (errore(1) / h(1)^2); 
loglog(h, errore_teorico, '--', 'LineWidth', 1.5, 'Color', 'k', 'DisplayName', 'Theoretical Slope O(h^2)');

% Some plot settings
grid on; box on;
ax = gca;
ax.GridLineStyle = ':';
ax.GridAlpha = 0.6;

xlabel('Grid Spacing h', 'FontSize', 12, 'FontWeight', 'bold');
ylabel('Error in L_2 Norm', 'FontSize', 12, 'FontWeight', 'bold');
title('Spatial Convergence Test (Jacobi vs Analytical)', 'FontSize', 14);
legend('Location', 'best', 'FontSize', 11);
