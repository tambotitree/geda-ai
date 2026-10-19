import pytest
from gedai.schematic_parser import parse_geda_schematic

def test_rf_amp_parsing():
    with open("examples/RF_Amp/schematics/rfamp-main.sch") as f:
        content = f.read()
    
    components = parse_geda_schematic(content)
    
    # Verify basic structure
    assert len(components) > 0
    assert any(c['type'] == 'component' for c in components)
    assert any(c['type'] == 'text' for c in components)
