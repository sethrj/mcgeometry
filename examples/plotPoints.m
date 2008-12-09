% amrOut.txt complexOut.txt meshOut.txt sphereOut.txt

readData = readtext('../complexOut.txt', '\t', '', '"','numeric-empty2zero');

figure(1)
clf

baseColors = {[1 0 0] [0 1 0] [0 0 1] [1 1 0] [1 0 1] [0 1 1]};
colors     = {};
for i = 0:5
   for j = 1:length(baseColors)
      colors{end+1} = baseColors{j} .* (1 - i/5);
   end
end

highestSurface = max(readData(:,1));
colorLen = length(colors);

for i = 1:highestSurface
   indices = readData(:,1) == i;
   plot3(readData(indices,2),readData(indices,3),readData(indices,4),...
      'MarkerEdgeColor',colors{mod(i - 1,colorLen) + 1},'Marker','.','MarkerSize',6,...
      'LineStyle','none')
   hold on
end
hold off
axis equal
