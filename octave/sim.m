
%
% similarity by cos
%
function s = sim_cos(a, b)
  if length(a) > length(b)
    b = [b linspace(0, 0, length(a)-length(b))];
  elseif length(a) < length(b)
    a = [a linspace(0, 0, length(b)-length(a))];
  end
  s = sum(a.*b)/(norm(a)*norm(b));
end

%
% similarity by Levenshtein Distance(Edit Distance)
%
function s = sim_edit(a, b)
  s = 1 - editdistance(a, b)/max(length(a), length(b));
end