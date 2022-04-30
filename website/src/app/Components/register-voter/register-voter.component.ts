import { Component, OnInit, ViewChild, ElementRef } from '@angular/core';
import { VotingService } from '../../service/voting.service';

@Component({
  selector: 'app-register-voter',
  templateUrl: './register-voter.component.html',
  styleUrls: ['./register-voter.component.scss']
})
export class RegisterVoterComponent implements OnInit {

  constructor(public votingservice : VotingService) { }
  @ViewChild('name') name: ElementRef;
  //@ViewChild('gender') gender: ElementRef;
  @ViewChild('dob') dob: ElementRef;
  @ViewChild('email') email: ElementRef;
  @ViewChild('phone') phone: ElementRef;
  @ViewChild('address') address: ElementRef;
  @ViewChild('pincode') pincode: ElementRef;
  selectedGender: string;
  ngOnInit(): void {

  }

  register(){
    let body = {
      "fullname" : this.name.nativeElement.value,
      "email" : this.email.nativeElement.value,
      "phone" : this.phone.nativeElement.value,
      "dob" : this.dob.nativeElement.value,
      "gender": this.selectedGender,
      "address" : this.address.nativeElement.value,
      "pincode" : this.pincode.nativeElement.value
    }
    this.votingservice.sendVoterData(body).then(s => {
      console.log(s)
      if(s["status"] == 200){
        let body1 = {
          "bloId" : "001",
          "boothId" : "001",
          "vuid" : s["vuid"]
        }
        console.log(body1);
        this.votingservice.registerVote(body1).then(s => {
          console.log(s);
        });
      }
    })

  }

}
