% Dati estratti dal tuo solutore
N = [16; 32; 64; 128];
errore = [0.00737541; 0.00171521; 0.000414652; 0.000101998];

% Calcolo del passo della griglia h
h = 1 ./ (N - 1);

% Fit lineare sui logaritmi per calcolare la pendenza sperimentale
% log(errore) = pendenza * log(h) + quota_origine
p = polyfit(log(h), log(errore), 1);
pendenza_sperimentale = p(1);

% Stampa il risultato a terminale
fprintf('=========================================\n');
fprintf('Pendenza sperimentale (Ordine): %.4f\n', pendenza_sperimentale);
fprintf('=========================================\n');

% --- GRAFICO DI CONVERGENZA ---
figure('Color', 'w');

% Plot dei dati sperimentali
loglog(h, errore, 'o-', 'LineWidth', 2, 'MarkerSize', 8, 'DisplayName', 'Errore L_2 Sperimentale');
hold on;

% Generiamo una retta di riferimento con pendenza esattamente pari a 2
% La trasliamo verticalmente per sovrapporla visivamente ai tuoi dati
errore_teorico = h.^2 * (errore(1) / h(1)^2); 
loglog(h, errore_teorico, '--', 'LineWidth', 1.5, 'Color', [0.5 0.5 0.5], 'DisplayName', 'Pendenza Teorica O(h^2)');

% Abbellimento del grafico
grid on; box on;
ax = gca;
ax.GridLineStyle = ':';
ax.GridAlpha = 0.6;

xlabel('Passo della griglia h', 'FontSize', 12, 'FontWeight', 'bold');
ylabel('Errore in Norma L_2', 'FontSize', 12, 'FontWeight', 'bold');
title('Test di Convergenza Spaziale (Jacobi vs Analitica)', 'FontSize', 14);
legend('Location', 'best', 'FontSize', 11);

% Forza i tick sugli assi a mostrare i valori di h corrispondenti a N
xticks(h);
xticklabels(cellstr(num2str(h, '%.3f')));