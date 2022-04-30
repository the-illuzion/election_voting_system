import { ComponentFixture, TestBed } from '@angular/core/testing';

import { RegisterVoterComponent } from './register-voter.component';

describe('RegisterVoterComponent', () => {
  let component: RegisterVoterComponent;
  let fixture: ComponentFixture<RegisterVoterComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ RegisterVoterComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(RegisterVoterComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
