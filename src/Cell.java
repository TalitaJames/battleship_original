import java.io.Serializable;

public class Cell implements Serializable{ 
	private ShipSegment shipSegment;
	private boolean hit;

	public Cell() {
		this.shipSegment = null;
		this.hit = false;
	}
	
	public boolean hasBeenHit() {
		return this.hit;
	}
	
	public boolean attack() {
		boolean success = false;
		if (this.shipSegment != null){
			this.shipSegment.attack();
			success=true;
		} 
		this.hit = true;
		return success;
	}
	
	public boolean isOccupied() {
		return this.shipSegment != null;
	}
	
	public void placeShipSegment(ShipSegment shipSegment) {
		if (!this.isOccupied()) {
			this.shipSegment = shipSegment;
		}
	}
	
	@Override
	public String toString() {
		if (!this.hit) {
			return ".";
		}
		else {
			if (!this.isOccupied()) {
				return "O";
			}
			else if (!this.shipSegment.getShip().sunk()) {
				return "X";
			}
			else {
				return this.shipSegment.toString();
			}
		}
	}
	
	public String displaySetup() {
		return this.isOccupied() ? this.shipSegment.toString() : ".";
	}
}
