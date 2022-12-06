use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::env;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, {
	Ok(io::BufReader::new(File::open(filename).expect("Unable to open file")).lines())
}

fn parse_line(line: &str)
{
	assert!(line.len() > 3);
	for i in 0..(line.len() - 3) {
		if line.chars().skip(i).take(1).eq(line.chars().skip(i + 1).take(1)) ||
		   line.chars().skip(i).take(1).eq(line.chars().skip(i + 2).take(1)) ||
		   line.chars().skip(i).take(1).eq(line.chars().skip(i + 3).take(1)) ||
		   line.chars().skip(i + 1).take(1).eq(line.chars().skip(i + 2).take(1)) ||
		   line.chars().skip(i + 1).take(1).eq(line.chars().skip(i + 3).take(1)) ||
		   line.chars().skip(i + 2).take(1).eq(line.chars().skip(i + 3).take(1)) {
			   continue;
		} else {
			println!("{}", i + 4);
			return;
		}
	}
}

fn main() {
	let args: Vec<String> = env::args().collect();

	if args.len() < 2 {
		eprintln!("Usage: {} <input file>", args[0]);
		std::process::exit(1);
	}

	for line in read_lines(&args[1]).expect("Unable to read lines") {
		parse_line(&line.expect("Invalid line"));
	}
}
