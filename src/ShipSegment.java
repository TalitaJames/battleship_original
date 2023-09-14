import java.io.Serializable;

public class ShipSegment implements Serializable{
	
	private final Ship ship;
	private boolean hit;

	public ShipSegment(final Ship ship) {
		this.ship = ship;
		this.hit = false;
	}

	public boolean hit() {
		return this.hit;
	}

	public void attack() {
		this.hit = true;
	}
	
	public Ship getShip() {
		return this.ship;
	}
	
	@Override
	public String toString() {
		return this.ship == null ? "?" : this.getShip().toString();
	}

}
