use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::env;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, {
	Ok(io::BufReader::new(File::open(filename).expect("Unable to open file")).lines())
}

fn parse_line(line: &str) -> u8
{
	assert!(line.len() % 2 == 0);
	let compartments: [&str; 2] = [&line[..(line.len()/2)], &line[(line.len()/2)..]];
	for c in compartments[0].chars() {
		match compartments[1].find(c) {
			Some(pos) => {
				let common = compartments[1].as_bytes()[pos];
				if common >= b'A' && common <= b'Z' {
					return common - b'A' + 27;
				} else {
					return common - b'a' + 1;
				}
			}
			None => continue
		}
	}

	assert!(true);

	0
}

fn main() {
	let args: Vec<String> = env::args().collect();
	let mut sum: u32 = 0;

	if args.len() < 2 {
		eprintln!("Usage: {} <input file>", args[0]);
		std::process::exit(1);
	}

	for line in read_lines(&args[1]).expect("Unable to read lines") {
		sum += parse_line(&line.expect("Invalid line")) as u32;
	}

	println!("{}", sum);
}
