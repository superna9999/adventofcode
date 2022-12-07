use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::env;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, {
	Ok(io::BufReader::new(File::open(filename).expect("Unable to open file")).lines())
}

struct FsFile {
	name: String,
	length: u32,
}

struct FsDir {
	name: String,
	dirs: Vec<FsDir>,
  	files: Vec<FsFile>,
}

fn parse_dir(lines: &mut io::Lines<io::BufReader<File>>, curdir: &mut FsDir) -> bool
{
	println!("# curdir {}", curdir.name);

	loop {
		let input: String;

		match lines.next() {
			Some(line) => {
				input = line.expect("Invalid line");
			}
			None => {
				return false;
			}
		}
		
		println!("# input {}", input);
		
		if input.starts_with("$") {
			if input.starts_with("$ cd ") {
				if input[5..].eq("/") {
					if curdir.name.eq("/") {
						continue;
					} else {
						return false;
					}
				} else if input[5..].eq("..") {
					return true;
				} else {
					match curdir.dirs
						    .iter()
						    .position(|dir| input[5..].eq(&dir.name)) {
						Some(pos) => {
							if parse_dir(lines, &mut curdir.dirs[pos]) == false {
								if curdir.name != "/" {
									return false;
								}
							}
						}
						None => {
							println!("Invalid dir {}", input);
							std::process::exit(1);
						}
					}
				}
			} else if input != "$ ls" {
				println!("Invalid command {}", input);
				std::process::exit(1);
			}
		} else {
			if input.starts_with("dir ") {
				println!("Folder {} new subdir {}", curdir.name, input);
				curdir.dirs.push(FsDir {
					name: input[4..].to_string(),
					dirs: Vec::new(),
					files: Vec::new(),
				}); 
			} else {
				let file: Vec<&str> = input.split(" ").collect();
				match file[0].to_string().parse() {
					Ok(length) => {
						println!("Folder {} new file {} length {}", curdir.name, file[1], length);
						curdir.files.push(FsFile {
							name: file[1].to_string(),
							length: length,
						});
					}
					Err(e) => {
						println!("Invalid length format {}", e);
						std::process::exit(1);
					}
				}
			}
		}
	}
}

fn print(curdir: FsDir, pos: u32)
{
	for i in 0..pos {
		print!("  ");
	}
	println!("- {} (dir)", curdir.name);

	for dir in curdir.dirs {
		print(dir, pos + 1);
	}

	for file in curdir.files {
		for i in 0..(pos + 1) {
			print!("  ");
		}
		println!("- {} (file, size={})", file.name, file.length);
	}
}


fn dirsize(curdir: FsDir) -> (u32, u32)
{
	let mut sum: u32 = 0;
	let mut thsum: u32 = 0;

	for dir in curdir.dirs {
		let (dirsum, dirthsum) = dirsize(dir);
		sum += dirsum;
		thsum += dirthsum;
		if dirsum <= 100000 {
			thsum += dirsum
		}
	}

	for file in curdir.files {
		sum += file.length;
	}

	(sum, thsum)
}

fn main() {
	let args: Vec<String> = env::args().collect();
	let mut root = FsDir {
		name: String::from("/"),
		dirs: Vec::new(),
		files: Vec::new(),
	};

	if args.len() < 2 {
		eprintln!("Usage: {} <input file>", args[0]);
		std::process::exit(1);
	}

	parse_dir(&mut read_lines(&args[1]).expect("Unable to read lines"), &mut root);

	println!("Total size: {:?}", dirsize(root));
}
