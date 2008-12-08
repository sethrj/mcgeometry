readData = readtext('../sphereOut.txt', '\t', '', '"','numeric-empty2zero');

figure(1)
clf
colors = {'r','g','b','c','m'};
for i = 1:5
   indices = readData(:,1) == i;
   plot3(readData(indices,2),readData(indices,3),readData(indices,4),...
      'MarkerEdgeColor',colors{i},'Marker','.','MarkerSize',6,...
      'LineStyle','none')
   hold on
end
hold off
axis equal
