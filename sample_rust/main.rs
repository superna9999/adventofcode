use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::env;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, {
	let file = File::open(filename)?;
	Ok(io::BufReader::new(file).lines())
}

fn main() {
	let args: Vec<String> = env::args().collect();
	let mut v: Vec<Vec<i32>> = Vec::new();
	let mut cur: usize = 0;

	v.push(Vec::new());

	if let Ok(lines) = read_lines(&args[1]) {
		for line in lines {
			if let Ok(ip) = line {
				println!("{}", ip);
			}
		}
	}
}
