use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::env;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, {
	Ok(io::BufReader::new(File::open(filename).expect("Unable to open file")).lines())
}

fn run_instructions(_crates: &mut Vec<Vec<char>>, line: &str)
{
	let c: usize = line[5..line.find(" from ").expect("missing from")].parse().unwrap();
	let f: usize = line[(line.find(" from ").expect("missing from") + 6)..line.find(" to ").expect("missing to")].parse().unwrap();
	let t: usize = line[(line.find(" to ").expect("missing to") + 4)..].parse().unwrap();

	for _i in 0..c {
		let unload = _crates[f - 1].pop().expect("bad crate pop");
		_crates[t - 1].push(unload);
	}
}

fn parse_crates(_crates: &mut Vec<Vec<char>>, line: &str)
{
	let chars = line.chars();
	let mut next: usize = usize::MAX;

	for (i, x) in chars.enumerate() {
		if x == '[' {
			next = i + 1;
		} else if next == i {
			let crate_num = i / 4;

			while _crates.len() < (crate_num + 1) {
				_crates.push(Vec::new());
			}

			_crates[crate_num].push(x);
		}
	}
}

fn main() {
	let args: Vec<String> = env::args().collect();
	let mut crates: Vec<Vec<char>> = Vec::new();
	let mut act: bool = false;

	if args.len() < 2 {
		eprintln!("Usage: {} <input file>", args[0]);
		std::process::exit(1);
	}

	for line in read_lines(&args[1]).expect("Unable to read lines") {
		let line = &line.expect("Invalid line");

		if line.len() == 0 {
			for c in &mut *crates {
				c.reverse();
			}

			act = true;
			continue;
		}

		match act {
			true => {
				run_instructions(&mut crates, &line);
			}
			false => {
				parse_crates(&mut crates, &line);
			}
		}
	}

	let mut result = String::from("");

	for c in &mut *crates {
		c.reverse();
		result.push(c[0]);
	}

	println!("{:?}", result);
}
