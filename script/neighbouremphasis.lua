-- Get atom 0's neighbours up to 16 Angstroms.
neighbours = sfoav.getAtomsNeighbours(0, 4.0)

-- Fade all atoms.
for i = 1, sfoav.atomCount() do
    r, g, b, a = sfoav.getAtomColour(i-1)
    sfoav.setAtomColour(i-1, r, g, b, 0.1)
end

-- Emphasise all neighbours to 0 withint 16 Angstroms.
for i = 1, #neighbours do
    r, g, b, a = sfoav.getAtomColour(neighbours[i]["index"])
    sfoav.setAtomColour(neighbours[i]["index"], r, g, b, 1.0);
end